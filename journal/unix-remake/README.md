OneShot Journal
===============

This is a Linux-compatible reimplementation of the Author's Journal, a element in the game [OneShot](http://oneshot-game.com).

Usage
-----

This journal requires PyQt5 to run. On Debian/Ubuntu-based systems, install via the following command:

    sudo apt-get install python3-pyqt5

This will install PyQt5 and all necessary files. Next, extract the images from the original OneShot journal program using the instructions in the following section, then use the following command to start the journal:

    python3 journal.py

Start OneShot and play normally.

Extracting the images
---------------------

First, aquire a copy of the Windows-version journal executable. Next, download [ResourcesExtract](http://www.nirsoft.net/utils/resources_extract.html) and run it, either via Wine or from a Windows install. Set the filename to the journal executable and the destination to the images directory in your local copy of this repo. Set the program to extract only bitmaps, then press Start. Once extraction completes run the included Python script `renameandconvert.py` (requires ImageMagick), which will create the required PNGs.
