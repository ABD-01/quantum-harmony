# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['main.py', 'app_logic.py', 'app_ui.py', 'PCAN_UDS_2013.py', 'PCAN_ISO_TP_2016.py', 'PCANBasic.py'],
    pathex=[],
    binaries=[],
    datas=[('aepl.ico', '.'), ('background.gif', '.'), ('PCANBasic.dll', '.'), ('PCAN-UDS.dll', '.'), ('PCAN-ISO-TP.dll', '.')],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    a.binaries,
    a.datas,
    [],
    name='main',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    upx_exclude=[],
    runtime_tmpdir=None,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
