import os
import json
import sys

import torch
from PIL import Image
from torchvision import transforms
import matplotlib.pyplot as plt
import  torchvision
from model import resnet34

from tqdm import tqdm
import torch.nn.functional as F



def main(img_name,ToDraw):

    device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

    data_transform = transforms.Compose(
        [transforms.Resize(256),
         transforms.Grayscale(num_output_channels=1),
         transforms.Grayscale(num_output_channels=3),
         transforms.CenterCrop(224),
         transforms.ToTensor(),
         transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])])


    # load image
    img_path = "../scripts/" + img_name #"test1.jpeg"  # 搜索这张图
    if not os.path.exists(img_path):
        return -1
    img = Image.open(img_path)
    if ToDraw > 0:
        plt.subplot(231)
        plt.imshow(img)
        plt.title("检索目标")
    # [N, C, H, W]
    img = data_transform(img)
    torchvision.utils.save_image(img, '../scripts/out.jpg')

    # expand batch dimension
    img = torch.unsqueeze(img, dim=0)

    # read class_indict
    json_path = '../scripts/3DFDataset.json'
    if not os.path.exists(json_path):
        return -2

    # with open(json_path, "r") as f:
    #     data_list = json.load(f)

    file = open(json_path, 'r')
    data_list = []
    view_list = []
    load_bar = tqdm(file.readlines(), file=sys.stdout)
    for line in load_bar:
        if line == '[\n' or line == '    [\n'  or line == '    ]\n' or line == ']':
            continue
        if line != '][\n' and line != '    ],\n':
            num_str = line[8:len(line)-2]
            num = json.loads(num_str)
            view_list.append(num)
            continue
        data_list.append(view_list[:])
        view_list.clear()
        load_bar.desc = "加载数据库中"



    data_tensor = torch.tensor(data_list).to(device)
    # create model
    model = resnet34().to(device)

    # load model weights
    weights_path = "../scripts/FresNet34.pth"
    if not os.path.exists(weights_path):
        return -3
    model.load_state_dict(torch.load(weights_path, map_location=device))

    # prediction
    model.eval()
    with torch.no_grad():
        # predict class
        output = model(img.to(device))
        output = F.normalize(output, p=2, dim=1)
        # best_similarity = -100.0
        sim_dic = {}
        predict_bar = tqdm(range(len(data_tensor)), file=sys.stdout)
        for i in predict_bar:
            index = torch.tensor([i]).to(device)
            view = torch.index_select(data_tensor, 0, index)
            view = F.normalize(view, p=2, dim=1)
            similarity = (output@view.t()).item()
            sim_dic[i] = similarity
            # if similarity > best_similarity:
            #     best_similarity = similarity
            #     best_index = i
            predict_bar.desc = "检索中"

    sim_order = sorted(sim_dic.items(), key=lambda x: x[1], reverse=True)

    # print(f"best_similarity = {best_similarity}")

    # load image
    list = os.listdir('../scripts/FDataset/photos')
    FileNameList = []
    SimList = []
    for i in range(5):
        thisindex = sim_order[i][0]
        if ToDraw > 0:
            Target_img_path = "../scripts/FDataset/photos/"+list[thisindex]
            img1 = Image.open(Target_img_path)
            plt.subplot(232+i)
            plt.imshow(img1)
            plt.title(f"检索结果{i+1}\n{list[thisindex]}\n相似度：{sim_order[i][1]*100:.3f}%")
        FileNameList.append(list[thisindex])
        SimList.append(sim_order[i][1])

    if (os.path.isfile("../scripts/FileNameList.json")):
        os.remove("../scripts/FileNameList.json")
    if (os.path.isfile("../scripts/SimList.json")):
        os.remove("../scripts/SimList.json")

    json_str = json.dumps(FileNameList, indent=0)
    with open('../scripts/FileNameList.json', 'a') as json_file:
        json_file.write(json_str)
    json_str = json.dumps(SimList, indent=0)
    with open('../scripts/SimList.json', 'a') as json_file:
        json_file.write(json_str)

    if ToDraw > 0:
        plt.subplots_adjust(left=None, bottom=None, right=None, top=0.75, wspace=1.8, hspace=1.5)
        plt.rcParams['font.sans-serif'] = ['FangSong']
        plt.rcParams['font.size'] = 15
        plt.suptitle("三维检索系统")
        plt.show()

    return 1