<?hh
    // This file should contain all your application specific settings, such as resource handling, module configuration etc.
    /**
     * Constants
     */
    const SITE_NAME = 'HydraCore Monitor Server';
    const AUTHOR = 'Ryan Howell';
    const REGISTER_SHUTDOWN = true;
    const MODE = 'MVC';

    /**
     * Pages
     */
    $hydraCoreSettings['pages'] = [
        'resources' => [
            'js' => [
                'main' => true
            ],
            'less' => [
                'fonts' => true
            ]
        ]
    ];

    /**
     * Hooks
     */
    $hydraCoreSettings['hooks'] = [
        'preReceive' => [
            'HC\Hooks\PreReceive\Lock' => true,
        ],
        'postReceive' => [
            'HC\Hooks\PostReceive\CompileResources' => [
                'languages' => [
                    'js' => true,
                    'scss' => true,
                    'less' => true
                ],
                'path' => '/resources/'
            ],
            'HC\Hooks\PostReceive\Unlock' => true
        ]
    ];