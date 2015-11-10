'use strict';

var gulp = require('gulp');
var path = require('path');
var proc = require('child_process');
var packager = require('electron-packager');

var srcDir = './app';
var cacheDir = path.join(srcDir, 'cache');
var compilerPath = path.join(srcDir, 'node_modules/.bin/electron-compile');
var packageJson = require('./' + path.join(srcDir, 'package.json'));

if(process.platform === 'win32')
	compilerPath += '.cmd';

gulp.task('default', ['package']);

gulp.task('compile', done => {
	var compile = proc.spawn(compilerPath, ['--target', cacheDir, srcDir], {stdio: 'inherit'});
	compile.on('close', done);
});

gulp.task('package', ['compile'], done => {
	packager({
		dir: srcDir,
		out: './release',
		name: packageJson.name,
		'app-version': packageJson.version,
		version: require('electron-prebuilt/package.json').version,
		arch: process.arch,
		platform: process.platform,
		overwrite: true,
		prune: true,
		asar: false
	}, (err, path) => done()
	);
});
