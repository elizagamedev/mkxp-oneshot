# -*- coding: utf-8 -*-

import os
import sys
import time

from PyQt5.QtCore import Qt, QThread, pyqtSignal, QRect, QRectF, QTimer
from PyQt5.QtWidgets import QApplication, QWidget, QLabel
from PyQt5.QtGui import QIcon, QPixmap, QPainter

if sys.platform == "win32": pipe_path = '\\\\.\\pipe\\oneshot-journal-to-game'
else: pipe_path = '/tmp/oneshot-pipe'

try: base_path = sys._MEIPASS
except AttributeError: base_path = os.path.abspath('.')

class WatchPipe(QThread):
    change_image = pyqtSignal(str)
    
    def run(self):
        while True:
            if os.path.exists(pipe_path): break
            else: time.sleep(0.1)

        pipe = open(pipe_path, 'r')
        pipe.flush()

        while True:
            message = os.read(pipe.fileno(), 256)
            if len(message) > 0:
                self.change_image.emit(message.decode())

            time.sleep(0.05)
            
class Journal(QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
                
        self.label = QLabel(self)
        self.change_image('default_en')
        
        self.setWindowFlags(self.windowFlags())
        self.setAttribute(Qt.WA_TranslucentBackground)
        self.setAttribute(Qt.WA_NoSystemBackground)
        self.setWindowTitle('')
        self.setMinimumSize(800, 600)
        self.setMaximumSize(800, 600)
        self.setGeometry(0, 0, 800, 600)
        self.show()
    
    def change_image(self, image):
        name, lang = image.split('_', 1)
        img = os.path.join(base_path, 'images', lang.upper(), '{}.png'.format(name))
        if not os.path.exists(img):
            img = os.path.join(base_path, 'images', '{}.png'.format(name))
            if not os.path.exists(img): return
        self.pixmap = QPixmap(img) # XXX Use QImage instead to support transparency!
        self.label.setPixmap(self.pixmap)

class Niko(QWidget):
    def __init__(self, *args, **kwargs):
        self.start_x, self.start_y = kwargs['start_x'], kwargs['start_y']
        del kwargs['start_x'], kwargs['start_y']
        
        super().__init__(*args, **kwargs)
        
        self.frames = [QPixmap(os.path.join(base_path, 'images', 'niko{}.png'.format(n)) for n in range(1,4))]

        self.setBackgroundRole(QPalette.Base)
        self.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)        

if __name__ == '__main__':
    app = QApplication(sys.argv)

    if len(sys.argv) == 3:
        # "Niko-leaves-the-screen" mode
        x, y = int(sys.argv[1]), int(sys.argv[2])
    
    else:
        # Author's Journal mode
        journal = Journal()
    
        thread = WatchPipe()
        thread.change_image.connect(journal.change_image)
        thread.start()
    
    app.exec_()
