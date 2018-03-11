# -*- coding: utf-8 -*-

import os, sys, time

from PyQt5.QtCore import Qt, QEvent, QThread, pyqtSignal, QRect, QRectF, QTimer, QPoint
from PyQt5.QtWidgets import QApplication, QWidget, QDesktopWidget, QLabel
from PyQt5.QtGui import QIcon, QPixmap, QPainter

if sys.platform == "win32":
	pipe_path = '\\\\.\\pipe\\oneshot-journal-to-game'
	import ctypes.wintypes
	buff = ctypes.create_unicode_buffer(ctypes.wintypes.MAX_PATH)
	ctypes.windll.shell32.SHGetFolderPathW(None, CSIDL_PERSONAL, None, SHGFP_TYPE_CURRENT, buf)
	documents_path = os.path.join(buf.value, 'My Games')
else:
	pipe_path = '/tmp/oneshot-pipe'
	documents_path = os.path.expanduser('~/Documents')

try: base_path = sys._MEIPASS
except AttributeError: base_path = os.path.abspath('.')

class WatchPipe(QThread):
	change_image = pyqtSignal(str)

	def run(self):
		while True:
			while True:
				if os.path.exists(pipe_path): break
				else: time.sleep(0.1)
			pipe = open(pipe_path, 'r')
			pipe.flush()

			while True:
				if not os.path.exists(pipe_path):
					break # Handle game quitting cleanup
				message = os.read(pipe.fileno(), 256)
				if len(message) > 0:
					self.change_image.emit(message.decode())

				time.sleep(0.05)

class AnimationTimer(QThread):
	next_frame = pyqtSignal()

	def run(self):
		while True:
			self.next_frame.emit()
			time.sleep(1.0 / 60)

class Journal(QWidget):
	def __init__(self, *args, **kwargs):
		super().__init__(*args, **kwargs)

		self.mousedown = False
		self.mousedownpos = QPoint(0, 0)

		self.label = QLabel(self)
		self.change_image('default_en')

		self.setAttribute(Qt.WA_TranslucentBackground)
		self.setMouseTracking(True)
		self.setWindowTitle(' ')
		self.setMinimumSize(800, 600)
		self.setMaximumSize(800, 600)
		self.setGeometry(0, 0, 800, 600)
		self.show()

	def mousePressEvent(self, event):
		self.mousedown = True
		self.mousedownpos = event.pos()

	def mouseReleaseEvent(self, event):
		self.mousedown = False

	def mouseMoveEvent(self, event):
		if event.buttons() == Qt.LeftButton:
			pos = event.pos()
			frameGm = self.frameGeometry()
			self.setGeometry(frameGm.x() + pos.x() - self.mousedownpos.x(), frameGm.y() + pos.y() - self.mousedownpos.y(), 800, 600)

	def change_image(self, image):
		name, lang = image.split('_', 1)
		if lang == 'en':
			img = os.path.join(base_path, 'images', '{}.png'.format(name))
		else:
			img = os.path.join(base_path, 'images', lang.upper(), '{}.png'.format(name))
		if not os.path.exists(img): return
		self.pixmap = QPixmap(img)
		self.label.setPixmap(self.pixmap)

class Niko(QWidget):
	def __init__(self, *args, **kwargs):
		self.x, self.y, self.start_y, self.app, self.thread = kwargs['x'], kwargs['y'], kwargs['y'], kwargs['app'], kwargs['thread']
		self.screen_width, self.screen_height = kwargs['screen_width'], kwargs['screen_height']
		del kwargs['x'], kwargs['y'], kwargs['screen_width'], kwargs['screen_height'], kwargs['app'], kwargs['thread']

		super().__init__(*args, **kwargs)

		self.setWindowFlags(Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint)
		self.setAttribute(Qt.WA_TranslucentBackground)
		self.setMinimumSize(48, 64)
		self.setMaximumSize(48, 64)

		self.label = QLabel(self)
		self.frames = [QPixmap(os.path.join(base_path, 'images', 'niko{}.png'.format(n))) for n in range(1,4)]
		self.label.setPixmap(self.frames[1])

		self.show()

		self.thread.start()

	def getFrame(self):
		if ((self.y - self.start_y) % 32 >= 16): return 1
		if ((self.y - self.start_y) % 64 >= 32): return 0
		else: return 2

	def update(self):
		self.label.setPixmap(self.frames[self.getFrame()])
		self.y += 2
		if self.y > self.screen_height:
			self.app.quit()
			return
		elif self.y > self.screen_height - 64:
			self.setMinimumSize(48, self.screen_height - self.y)
			self.resize(48, self.screen_height - self.y)
		self.move(self.x, self.y)

if __name__ == '__main__':
	app = QApplication(sys.argv)

	if len(sys.argv) == 3:
		# "Niko-leaves-the-screen" mode
		x, y = int(sys.argv[1]), int(sys.argv[2])
		screensize = app.primaryScreen().size()

		thread = AnimationTimer()

		niko = Niko(x = x, y = y, screen_width = screensize.width(), screen_height = screensize.height(), app = app, thread = thread)

		thread.next_frame.connect(niko.update)

	else:
		# Author's Journal mode
		journal = Journal()
		save_path = os.path.join(documents_path, 'Oneshot', 'save_progress.oneshot')
		if os.path.exists(save_path):
			with open(save_path, 'rb') as save:
				save.seek(-8, os.SEEK_END)
				lang = save.read().decode('utf-8')
				lang = lang[lang.find('[') + 1:lang.find(']')]
				if lang == 'en_US':
					lang = 'en'
				journal.change_image('save_' + lang)
		else:
			thread = WatchPipe()
			thread.change_image.connect(journal.change_image)
			thread.start()

	app.exec_()
