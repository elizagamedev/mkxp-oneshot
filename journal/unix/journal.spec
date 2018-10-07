# -*- mode: python -*-

block_cipher = None

a = Analysis(['journal.py'],
             pathex=['.'],
             binaries=[],
             datas=[('images', 'images'), ('qt.conf', '.')],
             hiddenimports=[],
             hookspath=[],
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          exclude_binaries=True,
          name='_______',
          debug=False,
          strip=False,
          upx=True,
          console=False,
          icon='assets/icon_journal.ico')
coll = COLLECT(exe,
               a.binaries,
               a.zipfiles,
               a.datas,
               strip=False,
               upx=True,
               name='_______')
app = BUNDLE(coll,
             name='_______.app',
             icon='assets/icon_journal.icns',
             bundle_identifier="APPL"
            )
