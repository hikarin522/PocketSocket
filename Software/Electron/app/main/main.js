'use strict';

import {app, BrowserWindow, crashReporter} from 'electron';
import path from 'path';

export default class MainWindow {
	constructor() {
		crashReporter.start();

		app.on('window-all-closed', () => {
			if (process.platform != 'darwin')
			app.quit();
		});

		app.on('ready', () => {
			const url = path.join('file://', __rendererdir, './index.html');
			this.createWindow(url, {width: 800, height: 600});

		});
	}

	createWindow(path, options) {
		this.window = new BrowserWindow(options);
		this.window.loadURL(path);
		if (!__isproduction)
			this.window.webContents.openDevTools();
		this.window.on('closed', () => {
			this.window = null;
		});
	}
};
