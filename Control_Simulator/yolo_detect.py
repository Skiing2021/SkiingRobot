import argparse
import time
from pathlib import Path

import cv2
import torch
import torch.backends.cudnn as cudnn
from numpy import random

import numpy as np

# Programmatically import yolov5 models
# ==========================================
#
# import importlib
# import sys
# import os
#
# spec_models = importlib.util.spec_from_file_location("models", os.path.join(__yolov5_path, "models", "__init__.py"))
# models = importlib.util.module_from_spec(spec_models)
# sys.modules[spec_models.name] = models
# spec_models.loader.exec_module(models)
#
# spec_utils = importlib.util.spec_from_file_location("utils", os.path.join(__yolov5_path, "utils", "__init__.py"))
# utils = importlib.util.module_from_spec(spec_utils)
# sys.modules[spec_utils.name] = utils
# spec_utils.loader.exec_module(utils)
#
# import models
# import utils
# ==========================================

from models.experimental import attempt_load
from utils.datasets import LoadStreams, LoadImages
from utils.general import check_img_size, non_max_suppression, apply_classifier, scale_coords, xyxy2xywh, \
    strip_optimizer, set_logging, increment_path
from utils.plots import plot_one_box
from utils.torch_utils import select_device, load_classifier, time_synchronized

class YoloDetector:

    def __init__(self, weights, device, img_size, conf_thres, iou_thres):

        # Initialize
        set_logging()
        self.device = select_device(device)
        self.half = self.device.type != 'cpu'  # half precision only supported on CUDA

        # Load model
        self.model = attempt_load(weights, map_location=self.device)  # load FP32 model
        self.imgsz = check_img_size(img_size, s=self.model.stride.max())  # check img_size
        if self.half:
            self.model.half()  # to FP16

        # Second-stage classifier
        self.classify = False
        if self.classify:
            self.modelc = load_classifier(name='resnet101', n=2)  # initialize
            self.modelc.load_state_dict(torch.load('weights/resnet101.pt', map_location=self.device)['model']).to(
                self.device).eval()

        # Get names and colors
        self.names = self.model.module.names if hasattr(self.model, 'module') else self.model.names
        # self.colors = [  # color in BGR
        #     [255, 0, 0],
        #     [0, 0, 255]
        # ]
        self.colors = [[random.randint(0, 255) for _ in range(3)] for _ in self.names]

        self.conf_thres = conf_thres
        self.iou_thres = iou_thres

    def Detect(self, cv2_img):
        img = cv2.resize(cv2_img, (self.imgsz, self.imgsz))
        img = np.asarray(img)
        img = img.transpose(2, 0, 1)
        img = torch.from_numpy(img).to(self.device)
        img = img.half() if self.half else img.float()  # uint8 to fp16/32
        img /= 255.0  # 0 - 255 to 0.0 - 1.0
        if img.ndimension() == 3:
            img = img.unsqueeze(0)

        # Inference
        pred = self.model(img)[0]

        # Apply NMS
        pred = non_max_suppression(pred, self.conf_thres, self.iou_thres, None, None)

        # Apply Classifier
        if self.classify:
            # pred = apply_classifier(pred, self.modelc, img, im0s)
            raise NotImplementedError

        return pred

