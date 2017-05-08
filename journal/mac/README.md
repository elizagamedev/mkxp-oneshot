OneShot Journal
===============

This is a Linux- and macOS-compatible reimplementation of the Author's Journal, a element in the game [OneShot](http://oneshot-game.com).

Usage
-----

This journal requires Python 3.x and PyQt5 to run. On macOS or Debian/Ubuntu-based systems, install via one of the following commands:

    brew install pyqt5                   # macOS (Homebrew)
    port install pyqt5                   # macOS (MacPorts)
    sudo apt-get install python3-pyqt5   # Ubuntu/Debian

This will install PyQt5 and all necessary files. Next, extract the images from the original OneShot journal program using the instructions in the following section, then use the following command to start the journal:

    python3 journal.py

Start OneShot and play normally.

Building the app bundle
-----------------------

To create the macOS app bundle, PyInstaller is required.  Install via one of the following commands:

	brew install pyinstaller   # macOS (Homebrew)
	port install pyinstaller   # macOS (MacPorts)
	
*Note: If you plan to use Python 3.6, at the time of this writing, the stable release (3.2.1) of PyInstaller only supports up to Python 3.5, however the [development build](http://www.pyinstaller.org/downloads.html) implements Python 3.6 support.*

Now that everything is installed, let's create our app bundle:

	pyinstaller journal.spec --onefile --windowed

This should collect all the required runtime files and create the bundle in `dist/_______.app`.

Extracting the images
---------------------

First, aquire a copy of the Windows-version journal executable. Next, download [ResourcesExtract](http://www.nirsoft.net/utils/resources_extract.html) and run it, either via Wine or from a Windows install. Set the filename to the journal executable and the destination to the images directory in your local copy of this repo. Set the program to extract only bitmaps, then press Start. Once extraction completes run the included Python script `renameandconvert.py` (requires ImageMagick), which will create the required PNGs.
