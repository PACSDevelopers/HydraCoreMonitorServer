<?hh
    // This file should contain all your application specific settings, such as resource handling, module configuration etc.
    /**
     * Constants
     */
    const SITE_NAME = 'PACS Tools Monitor';
    const AUTHOR = 'PACS Tools';
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
        ],
        'rewrites' => [
            '@^/domains/(?<id>\d+)$@' => '/domains/domain.hh',
            '@^/servers/(?<id>\d+)$@' => '/servers/server.hh',
            '@^/databases/(?<id>\d+)$@' => '/databases/database.hh',
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
        ],
        'cron' => [
            'HCMS\Hooks\Cron\ProcessDatabases' => [
                'microtime' => 60
            ],
            'HCMS\Hooks\Cron\ProcessDomains' => [
                'microtime' => 60
            ],
            'HCMS\Hooks\Cron\ProcessServers' => [
                'microtime' => 60
            ],
        ]
    ];