from PyQt5 import QtWidgets,QtCore,QtGui
from LibMWCapture import *
import numpy as np
import cv2

class QRenderWidget(QtWidgets.QWidget):
    def __init__(self, parent=None):
        super(QRenderWidget, self).__init__()
        vbox = QtWidgets.QVBoxLayout()
        self.img_frame = QtWidgets.QLabel()
        vbox.addWidget(self.img_frame)
        self.setLayout(vbox)

    def open_render(self, fourcc, width, height):
        self.m_width = width
        self.m_height = height
        self.m_fourcc = fourcc
        print("fourcc = {}, width = {}, height{}".format(fourcc, width, height))

    def put_frame(self, pbframe):
        # self.m_data = pbframe #copy.deepcopy(pbframe)
        cvimg_raw = np.frombuffer(pbframe.raw, dtype=np.uint8).reshape(self.m_height, self.m_width, 3)
        cvimg = cv2.resize(cvimg_raw, None, fx=0.5, fy=0.5)
        qimg = QtGui.QImage(cvimg.data,
                            cvimg.shape[1],
                            cvimg.shape[0],
                            QtGui.QImage.Format_RGB888).rgbSwapped()
        self.img_frame.setPixmap(QtGui.QPixmap.fromImage(qimg))
        self.update()

class MWControlWidget(QtWidgets.QWidget):
    def __init__(self, capture=None, parent=None):
        super(MWControlWidget, self).__init__(parent=parent)
        self.capture = capture
        vbox = QtWidgets.QVBoxLayout()

        grid = QtWidgets.QGridLayout()
        self.qlb_contrast = QtWidgets.QLabel("Contrast")
        self.qsl_contrast = QtWidgets.QSlider(QtCore.Qt.Horizontal)
        self.qsl_contrast.setRange(50, 200)
        self.qsl_contrast.setValue(100)
        self.slot_pixel_contrast(self.qsl_contrast.value())
        self.qsl_contrast.valueChanged.connect(self.slot_pixel_contrast)
        grid.addWidget(self.qlb_contrast, 0, 0)
        grid.addWidget(self.qsl_contrast, 0, 1)

        self.qlb_brightness = QtWidgets.QLabel("Brightness")
        self.qsl_brightness = QtWidgets.QSlider(QtCore.Qt.Horizontal)
        self.qsl_brightness.setRange(-100, 100)
        self.qsl_brightness.setValue(0)
        self.slot_pixel_brightness(self.qsl_brightness.value())
        self.qsl_brightness.valueChanged.connect(self.slot_pixel_brightness)
        grid.addWidget(self.qlb_brightness, 1, 0)
        grid.addWidget(self.qsl_brightness, 1, 1)

        self.qlb_saturation = QtWidgets.QLabel("Saturation")
        self.qsl_saturation = QtWidgets.QSlider(QtCore.Qt.Horizontal)
        self.qsl_saturation.setRange(0, 200)
        self.qsl_saturation.setValue(100)
        self.slot_pixel_saturation(self.qsl_saturation.value())
        self.qsl_saturation.valueChanged.connect(self.slot_pixel_saturation)
        grid.addWidget(self.qlb_saturation, 2, 0)
        grid.addWidget(self.qsl_saturation, 2, 1)

        self.qlb_hue = QtWidgets.QLabel("Hue")
        self.qsl_hue = QtWidgets.QSlider(QtCore.Qt.Horizontal)
        self.qsl_hue.setRange(-90, 90)
        self.qsl_hue.setValue(0)
        self.slot_pixel_hue(self.qsl_hue.value())
        self.qsl_hue.valueChanged.connect(self.slot_pixel_hue)
        grid.addWidget(self.qlb_hue, 3, 0)
        grid.addWidget(self.qsl_hue, 3, 1)

        self.qcb_deinterlace = QtWidgets.QComboBox()
        self.qcb_deinterlace.addItems(["Weave", "Blend", "Top", "Bottom"])
        self.qcb_deinterlace.currentTextChanged.connect(self.slot_deinterlace_mode)
        self.qcb_deinterlace.setCurrentIndex(1)
        grid.addWidget(QtWidgets.QLabel("Deinterlace"), 4, 0)
        grid.addWidget(self.qcb_deinterlace, 4, 1)

        self.qcb_color_fmt = QtWidgets.QComboBox()
        self.qcb_color_fmt.addItems(["Unknown", "RGB", "BT601", "BT709", "BT2020", "BT2020c"])
        self.qcb_color_fmt.currentTextChanged.connect(self.slot_color_format)
        grid.addWidget(QtWidgets.QLabel("ColorFmt"), 5, 0)
        grid.addWidget(self.qcb_color_fmt, 5, 1)

        self.qcb_quantization_range = QtWidgets.QComboBox()
        self.qcb_quantization_range.addItems(["Unknown", "Full", "Limited"])
        self.qcb_quantization_range.currentTextChanged.connect(self.slot_quantization_range)
        grid.addWidget(QtWidgets.QLabel("QuantRange"), 6, 0)
        grid.addWidget(self.qcb_quantization_range, 6, 1)

        self.qcb_saturation_range = QtWidgets.QComboBox()
        self.qcb_saturation_range.addItems(["Unknown", "Full", "Limited", "Extended Gamut"])
        self.qcb_saturation_range.currentTextChanged.connect(self.slot_saturation_range)
        grid.addWidget(QtWidgets.QLabel("QuantRange"), 7, 0)
        grid.addWidget(self.qcb_saturation_range, 7, 1)
        vbox.addLayout(grid)

        self.qpb_reset = QtWidgets.QPushButton("Reset Default")
        self.qpb_reset.clicked.connect(self.slot_button_reset)
        vbox.addWidget(self.qpb_reset)

        # qpb_save = QtWidgets.QPushButton("Save Image")
        # qpb_save.clicked.connect(self.slot_button_save)
        # vbox.addWidget(qpb_save)
        self.setLayout(vbox)

    def slot_pixel_contrast(self, value):
        self.qlb_contrast.setText("Contrast {:03d}".format(value))
        self.capture.m_contrast = value
        print("slot pixel contrast = {}".format(value))

    def slot_pixel_brightness(self, value):
        self.qlb_brightness.setText("Brightness {:03d}".format(value))
        self.capture.m_brightness = value
        print("slot pixel brightness = {}".format(value))

    def slot_pixel_saturation(self, value):
        self.qlb_saturation.setText("Saturation {:03d}".format(value))
        self.capture.m_saturation = value
        print("slot pixel saturation = {}".format(value))

    def slot_pixel_hue(self, value):
        self.qlb_hue.setText("Hue {:03d}".format(value))
        self.capture.m_hue = value
        print("slot pixel hue = {}".format(value))

    def slot_deinterlace_mode(self, txt):
        dict = {"Weave": MWCAP_VIDEO_DEINTERLACE_WEAVE,
                "Blend": MWCAP_VIDEO_DEINTERLACE_BLEND,
                "Top": MWCAP_VIDEO_DEINTERLACE_TOP_FIELD,
                "Bottom": MWCAP_VIDEO_DEINTERLACE_BOTTOM_FIELD}
        self.capture.m_deinterlace_mode = dict[txt]
        print("slot deinterlace mode: {}-{}".format(txt, self.capture.m_deinterlace_mode))

    def slot_color_format(self, txt):
        dict = {"Unknown": MWCAP_VIDEO_COLOR_FORMAT_UNKNOWN,
                "RGB": MWCAP_VIDEO_COLOR_FORMAT_RGB,
                "BT601": MWCAP_VIDEO_COLOR_FORMAT_YUV601,
                "BT709": MWCAP_VIDEO_COLOR_FORMAT_YUV709,
                "BT2020": MWCAP_VIDEO_COLOR_FORMAT_YUV2020,
                "BT2020c": MWCAP_VIDEO_COLOR_FORMAT_YUV2020C}
        self.capture.m_color_format = dict[txt]
        print("slotColorFormat: {}".format(self.capture.m_color_format))

    def slot_quantization_range(self, txt):
        dict = {"Unknown": MWCAP_VIDEO_QUANTIZATION_UNKNOWN,
                "Full": MWCAP_VIDEO_QUANTIZATION_FULL,
                "Limited": MWCAP_VIDEO_QUANTIZATION_LIMITED}
        self.capture.m_quantization_range = dict[txt]
        print("slot quantization range: {}-{}".format(txt, self.capture.m_quantization_range))

    def slot_saturation_range(self, txt):
        dict = {"Unknown": MWCAP_VIDEO_SATURATION_UNKNOWN,
                "Full": MWCAP_VIDEO_SATURATION_FULL,
                "Limited": MWCAP_VIDEO_SATURATION_LIMITED,
                "Extended Gamut": MWCAP_VIDEO_SATURATION_EXTENDED_GAMUT}
        self.capture.m_saturation_range = dict[txt]
        print("slot saturation range: {}-{}".format(txt, self.capture.m_saturation_range))

    def slot_button_reset(self):
        self.qsl_contrast.setValue(100)
        self.qsl_brightness.setValue(0)
        self.qsl_saturation.setValue(100)
        self.qsl_hue.setValue(0)
        self.qcb_deinterlace.setCurrentText("Blend")
        self.qcb_color_fmt.setCurrentText("Unknown")
        self.qcb_quantization_range.setCurrentText("Unknown")
        self.qcb_saturation_range.setCurrentText("Unknown")

if __name__ == '__main__':
    import sys
    app = QtWidgets.QApplication(sys.argv)
    w = MWControlWidget()
    w.show()
    sys.exit(app.exec_())