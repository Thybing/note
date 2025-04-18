# yolo目标检测入门实战笔记

在GPT的帮助下快速上手训练了一个模型并成功部署在树莓派上。
做一份备忘笔记。

## 模型训练

### 准备数据集
首先，准备数据集，拍摄一些要识别的物体图片

### 标注图片
对于刚刚的数据集，我们需要标注出要识别的物体在图片中出现的位置。
每张图都有一个标注文件，其中yolo的txt标注文件的内容格式为
```
<class_id> <x_center> <y_center> <width> <height>
```
其中的坐标值都是归一化到[0,1]的

可以使用标注工具[LabelImg](https://github.com/HumanSignal/labelImg)

这里我使用了百度的[easydata](https://ai.baidu.com/easydata/)来进行智能标注，节省工作量。
复杂的任务暂时没试过效果，我自己拍的一分类的数据集比较简单，效果还是挺不错的，半小时打好了七百张。

### 文件目录结构整理
将刚刚的数据集分成三类，训练集，验证集和测试集。注意这三部分交集为空集，即同一个数据只能出现在其中一个中。
比较常见的划分为4:1或7:3或者6:2:2

将目录整理为如下
```
/dataset
|- train/
|  |- images/
|  |- labels/
|
|- valid/
|  |- images/
|  |- labels/
|
|- test/
|  |- images/
|  |- labels/
|
|- data.yaml
```

其中images存储图片，labels存储图片对应的yolo标注。

data.yaml是数据描述的配置文件，类似于
```yaml
train: ./train/images
val: ./valid/images
test: ./test/images

nc: 1
names: ['dog']
```
train val test 分别是训练集，验证集，测试集
nc是分类数量，names是label名称

### 配置训练环境
#### 首先安装好Docker

#### 安装NVIDIA Container Toolkit以获取Docker的GPU支持。
运行以下的命令以测试，如果看到了显卡信息，就说明我们已经可以在容器中使用显卡了。
```bash
docker run --gpus all nvidia/cuda:12.3.2-base-ubuntu22.04 nvidia-smi
```

#### 拉取Ultralytics提供的YOLO官方镜像
```bash
docker pull ultralytics/ultralytics:latest
```
这个镜像内置了：
- PyTorch + CUDA + cuDNN
- Ultralytics YOLO (兼容 YOLOv3 / v5 / v8)
- OpenCV 等常用依赖

#### 启动Docker容器
为了数据持久化，我们使用-v参数将工作目录挂载到容器中，例如
```bash
docker run --gpus all -it --rm -v D:/yolo-demo:/workspace/ ultralytics/ultralytics:latest
```
将"D:/yolo-demo"路径挂载到了容器内的/workspace/

### 开始训练模型
在挂载的目录里新建一个output文件夹，将训练的输出都放在output文件夹中以持久化数据。
可以通过添加 project 和 name 参数来自定义输出路径

开始训练，这里我们选用yolov3模型
```bash
yolo task=detect mode=train model=yolov3.yaml data=/workspace/dataset/data.yaml epochs=120 imgsz=320 batch=16 project=/workspace/output name=yolov3-demo
```

遇到了报错
```
RuntimeError: unable to write to file </torch_187_423146950_5>: No space left on device (28)
```
查询后得知这个错误通常是由于容器共享内存的空间不足引起的。
一个简单的解决方案为通过设置 --shm-size 参数来分配更多共享内存，在启动容器时加入参数。
```bash
docker run --gpus all -it --rm --shm-size=4g -v D:/yolo-demo:/workspace/ ultralytics/ultralytics:latest
```

再次开始训练

训练完成后我们可以在output/yolov3-demo/路径下找到训练的结果。
其中有
* weights/best.pt（最优模型）
* weights/last.pt（最后一轮模型）
* results.png（训练过程曲线图）
* opt.yaml（训练参数记录）
* train_batch*.jpg（训练时可视化图片）
* 其它日志文件

其中这两个pt文件就是我们需要的模型。

**使用测试集进行测试**
打开data.yaml，将val的值改为test的值，然后运行
(应该有什么方法能直接去跑测试集吧？懒得找了，最简单的方法就是把验证集的路径指向测试集，然后用以下命令跑一遍验证集(现在实际上是测试集))
```bash
yolo task=detect mode=val model=output/yolov3-demo/weights/best.pt data=/workspace/dataset/data.yaml imgsz=320 project=/workspace/output name=yolov3-demo-test
```
(别忘了把yaml文件的val改回去，否则下次练模型就把测试集当验证集了)

在/output/yolov3-demo-test/目录下可以查看到结果图，没有问题的话模型就训练好了。
我们接下来可以把它部署在树莓派上。

附一个最终的目录结构
```
挂载点(例如在win下是D:/yolo-demo,在容器内是/workspace)
|- dataset/
|  |- train/
|  |  |- images/
|  |  |  |- (训练集图片xxx.png)
|  |  |- labels/
|  |  |  |- (训练集标注xxx.txt)
|  |- valid/
|  |  |- images/
|  |  |  |- (验证集图片xxx.png)
|  |  |- labels/
|  |  |  |- (验证集标注xxx.txt)
|  |- test/
|  |  |- images/
|  |  |  |- (测试集图片xxx.png)
|  |  |- labels/
|  |  |  |- (测试集标注xxx.txt)
|  |- data.yaml
|- output/
|  |- yolov3-demo/
|  |  |- weights
|  |  |  |- best.pt
|  |  |  |- last.pt
|  |  |- (其它训练日志)
|  |- yolov3-demo-test/
|  |  |- (测试集测试结果图及日志)
```

## 将模型部署到树莓派上

这里我用的是树莓派5,8G版本 + 官方的树莓派系统

### 环境准备

```sh
sudo apt update && sudo apt upgrade -y # 更新系统

sudo apt install python3 python3-pip -y # 安装py和pip

# 创建py的虚拟环境，防止和系统python冲突(要记住创建虚拟环境的位置，以便下次可以用source再次进入)
sudo apt install python3-venv -y
python3 -m venv yolovenv
source yolovenv/bin/activate

# 安装依赖: wheel、setuptools
pip install --upgrade pip wheel setuptools

# 安装ultralytics
pip install ultralytics
```

运行yolo，能看到ultralytics帮助信息，说明环境配置成功
```sh
yolo
```

### 模型部署
首先进入我们刚刚创建好的虚拟环境
```sh
source yolovenv/bin/activate
```
将模型拷贝到树莓派上，例如放到 yolo-demo/model/best.pt

拷一张测试用图片到树莓派，假如保存到 yolo-demo/test/000.png

创建一个output文件夹

运行:
```sh
yolo task=detect mode=predict model=model/best.pt source=test/000.png project=output name=rpi_test
```
推理结果会出现在output/rpi_test/

### 调用摄像头实现实时推理
我们使用python来直接写一个脚本实现调用摄像头实时推理[(见末尾附页)](#python-capture-detect)

### 换个更轻量的模型

运行了刚刚的模型，树莓派5需要两三秒才能跑一帧图像(没有装官方加速棒)。

在嵌入式环境下，可以选择更加轻量的模型，如
yolov5n，yolov3-tiny，yolov8n

这里选用了ultralytics最新版轻量模型yolov8n
在之前的基础上无需任何其它配置

在之前训练模型的时候，将训练的命令换成
```sh
yolo task=detect mode=train model=yolov8n.yaml data=/workspace/dataset/data.yaml epochs=120 imgsz=320 batch=16 project=/workspace/output name=yolov8-demo
```
只有model参数改成了yolov8n.yaml

用这个新训练的模型的best.pt替换掉刚刚yolov3训练出来的模型

再次启动py脚本，帧率提升到了11帧左右，足以完成一些实时性不高的任务。

## 附页

### 附：python调用摄像头和模型实现实时推理
<a id="python-capture-detect"></a>
```python
import cv2
from ultralytics import YOLO

# 加载YOLO模型
model = YOLO("model/best.pt")

# 打开摄像头
cap = cv2.VideoCapture(0)
cap.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)
cap.set(cv2.CAP_PROP_FPS, 30)

while True:
    ret, frame = cap.read()
    if not ret:
        break

    # 推理当前帧
    results = model(frame, verbose=False)[0]

    # 画框
    for box in results.boxes:
        x1, y1, x2, y2 = map(int, box.xyxy[0])
        conf = float(box.conf[0])
        cls = int(box.cls[0])
        label = f"{model.names[cls]} {conf:.2f}"
        cv2.rectangle(frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
        cv2.putText(frame, label, (x1, y1 - 5), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,255,0), 1)

    # 显示
    cv2.imshow("YOLO Camera", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
```
