# Skiing Robot auto control simulator

This project is used to prototype and test the auto control algorithm.

#### Recommended test video
| video_id | URL |
| ---- | ---- |
| vid25 | https://www.youtube.com/watch?v=8GwpubtvueM |
| vid32 | https://www.youtube.com/watch?v=M4XbJqRO00s |
| vid38 | https://www.youtube.com/watch?v=JsJY9U74l7w |

## Requirements

- Python 3.8
  - [PyTorch 1.7](https://pytorch.org/get-started/locally/)
  - opencv-python
- [YOLOv5](https://github.com/ultralytics/yolov5)

  

## Getting started

1. Install PyTorch 1.7

   ###### with CUDA-enabled GPU:

   ```shell
   pip install torch===1.7.1+cu110 torchvision===0.8.2+cu110 torchaudio===0.7.2 -f https://download.pytorch.org/whl/torch_stable.html
   ```

   ###### CPU only:

   ```shell
   pip install torch==1.7.1+cpu torchvision==0.8.2+cpu torchaudio===0.7.2 -f https://download.pytorch.org/whl/torch_stable.html
   ```

   For further installation instructions, please visit [Start Locally | PyTorch](https://pytorch.org/get-started/locally/)

   

2. Install opencv-python

   ```shell
   pip install opencv-python
   ```



3. Clone **this** repo to a directory of your choice

    ```shell
    git clone https://github.com/Skiing2021/SkiingRobot.git
    ```

4. Clone **yolov5** repo to **another** directory of your choice and install yolov5 dependencies

    ```shell
    git clone https://github.com/ultralytics/yolov5.git
    cd yolov5
    pip install -r requirements.txt
    ```

    Note that this repo will not run without yolov5 repo



5. Set environment variable `PYTHONPATH` to `yolov5` root directory


6. Run `python main.py --weights ../Models/exp1-yolov5s.pt --source tese_video/vid0.MOV` 

   or, you can open `Control_Simulator` directory with PyCharm
