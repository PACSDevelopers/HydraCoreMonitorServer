<?hh
    // This file is the entry point for your settings, require more files here for the settings to be applied
    require_once(__DIR__ . '/application.hh');
    if(file_exists(__DIR__ . '/server.hh')) {
        require_once(__DIR__ . '/server.hh');
    }
    if(file_exists(__DIR__ . '/bower.hh')) {
        require_once(__DIR__ . '/bower.hh');
    }
    