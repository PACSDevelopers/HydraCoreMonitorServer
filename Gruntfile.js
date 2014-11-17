module.exports = function(grunt) {

    // Project configuration.
    grunt.initConfig({
        pkg: grunt.file.readJSON('package.json'),
        wiredep: {
            target: {
                src: [
                    'tmp/bower/bower.html'
                ],
                options: {
                    dependencies: true,
                    devDependencies: true,
                    overrides: {
                        webshim: {
                            main: [
                                'js-webshim/minified/polyfiller.js'
                            ]
                        },
                        pleasejs: {
                            main: [
                                'dist/Please.js'
                            ]
                        }
                    }
                }
            }
        }
    });
    
    grunt.loadNpmTasks('grunt-wiredep');
};