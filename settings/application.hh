<?hh
    // This file should contain all your application specific settings, such as resource handling, module configuration etc.
    /**
     * Constants
     */
    const SITE_NAME = 'HydraCore Monitor Server';
    const APP_VERSION = '0.0.1';
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
        ],
        'rewrites' => [
            '@^/domains/(?<id>\d+)$@' => '/domains/domain.hh',
            '@^/servers/(?<id>\d+)$@' => '/servers/server.hh',
            '@^/databases/(?<id>\d+)$@' => '/databases/database.hh',
            '@^/downloads/backups/(?<id>\d+)$@' => '/downloads/backups.hh',
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
            'HC\Hooks\PostReceive\Unlock' => true,
            'HC\Hooks\PostReceive\GenerateErrorPages' => true
        ],
        'cron' => [
            'HCMS\Hooks\Cron\ProcessDatabases' => [
                'microtime' => 300
            ],
            'HCMS\Hooks\Cron\ProcessDomains' => [
                'microtime' => 300
            ],
            'HCMS\Hooks\Cron\ProcessServers' => [
                'microtime' => 300
            ],
            'HCMS\Hooks\Cron\ProcessBackups' => [
                'microtime' => 3600
            ],
            'HCMS\Hooks\Cron\ProcessCleanup' => [
                'microtime' => 60
            ],
            'HCMS\Hooks\Cron\ProcessManualBackups' => [
                'microtime' => 60
            ],
            'HCMS\Hooks\Cron\ProcessTransfers' => [
                'microtime' => 60
            ],
            'HCMS\Hooks\Cron\ProcessVault' => [
                'microtime' => 120
            ],
            'HCMS\Hooks\Cron\ProcessVaultCleanup' => [
                'microtime' => 3600
            ],
        ]
    ];