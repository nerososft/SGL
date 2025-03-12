# 连续图像滤镜并生成视频

资源轨道1: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;｜----图片1----｜

资源轨道2:   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;     ｜---------------图片2------------｜

资源轨道3:     &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;         ｜------图片3---------｜

资源轨道4:  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; ｜---------------视频1--------｜

滤镜轨道1:  &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;    ｜---滤镜1---｜

滤镜轨道2:    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;          ｜------滤镜2---------｜

1. 分帧信息

``` json
"frames": [
    {
        "cnt": 0,
        "images": [
            {
                "url": "/data/tmp/1.png",
                "position": {
                    "x": 0,
                    "y": 0
                },
                "size": {
                    "width" : 1024,
                    "height" : 768
                },
                "rotate" : {
                    "x" : 0,
                    "y" : 0,
                    "z" : 0
                }
            }
            {
                "url": "/data/tmp/2.png",
                "position": {
                    "x": 100,
                    "y": 100
                },
                "size": {
                    "width" : 100,
                    "height" : 100
                },
                "rotate" : {
                    "x" : 10,
                    "y" : 0,
                    "z" : 0
                }
            }
            ...
        ],
        "filters": [
            {
                "name": "gaussian_blur",
                "param" : {
                    "radius": 1
                }
            },
            {
                "name": "rotate",
                "param" : {
                    "angle_x": 1
                }
            }
            ...
        ]
    },
     {
        "cnt": 1,
        "images": [
            {
                "url": "/data/tmp/1.png",
                "position": {
                    "x": 0,
                    "y": 0
                },
                "size": {
                    "width" : 1024,
                    "height" : 768
                },
                "rotate" : {
                    "x" : 0,
                    "y" : 0,
                    "z" : 0
                }
            }
            {
                "url": "/data/tmp/2.png",
                "position": {
                    "x": 100,
                    "y": 100
                },
                "size": {
                    "width" : 100,
                    "height" : 100
                },
                "rotate" : {
                    "x" : 10,
                    "y" : 0,
                    "z" : 0
                }
            }
            ...
        ],
        "filters": [
            {
                "name": "gaussian_blur",
                "param" : {
                    "radius": 2
                }
            },
            {
                "name": "rotate",
                "param" : {
                    "angle_x": 2
                }
            }
            ...
        ]
    }
] 
```