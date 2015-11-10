'use strict';

var path = require('path');
var compile = require('electron-compile');

global.__basedir = __dirname;

global.__isproduction = path.parse(process.argv[0]).name === require('./package.json').name;
global.__isasar = path.extname(__basedir) === '.asar';
global.__unpackeddir = path.join(__basedir, __isasar ? '../app.asar.unpacked' : '');

global.__maindir = path.join(__basedir, 'main');
global.__rendererdir = path.join(__basedir, 'renderer');
global.__dlldir = path.join(__unpackeddir, 'dll');

var cacheDir = path.join(__basedir, 'cache');

if (__isproduction) {
	compile.initForProduction(cacheDir);
} else {
	compile.initWithOptions({cacheDir: cacheDir});
}

var main = require(path.join(__maindir, 'main'));
new main();
