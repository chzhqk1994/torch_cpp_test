# torch_cpp_test

1. ONNX DNN inference bug: (-215:Assertion failed) inputs.size() in function 'getMemoryShapes'

  1) onnx 모델 load 과정에서 위 와 같은 에러가 발생한다면 torch 1.12 가 아닌 다른 버전을 사용해서 onnx 모델을 만들면 해결된다 (ex torch 1.10 or 1.11)
  2) 또는 export.py 에서 아래 내용을 False 로 변경
    do_constant_folding=True,  # WARNING: DNN inference with torch>=1.12 may require do_constant_folding=False 


2. OpenCV(4.4.0) onnx_importer.cpp:1410: error: (-2:Unspecified error) in function 'void cv::dnn::dnn4_v20200609::ONNXImporter::populateNet(cv::dnn::dnn4_v20200609::Net)'

  위 와 같은 에러는 Upsampling 레이어에 의한 것이라고 하는데 opencv 버전을 4.5+ 로 업그레이드 하면 버그가 사라짐
  참조 : https://codeantenna.com/a/ofKLdfMAba