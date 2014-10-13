<?php

    namespace HCMS;

    /**
     * Class Core
     *
     * This class pulls all the settings and creates the site object.
     */
    class Core
    {
        protected $settings = [];

        // Construct the core based on settings
        public function __construct($settings = [])
        {
            $this->settings = \HC\Core::parseOptions($settings, $this->settings);

            spl_autoload_register('\HCMS\Core::autoLoader');

            if(PROTOCOL === 'https') {
                ini_set('session.use_only_cookies', 1);
                ini_set('session.cookie_secure', 1);
            }

            // Bind Session Handler
            $handler = new Session();
            session_set_save_handler($handler, true);
            \HC\User::startSession();

            return true;
        }

        /**
         * @param string $class
         */
        public static function autoLoader($class)
        {
            switch ($class) {
                /* Usable classes */
                case 'HCMS\Domain':
                    require_once(HC_APPLICATION_LOCATION . '/modules/data/domain.class.php');
                    break;
                case 'HCMS\Server':
                    require_once(HC_APPLICATION_LOCATION . '/modules/data/server.class.php');
                    break;
                case 'HCMS\Database':
                    require_once(HC_APPLICATION_LOCATION . '/modules/data/database.class.php');
                    break;
                
                /* System classes */
                case 'HCMS\Session':
                    require_once(HC_APPLICATION_LOCATION . '/modules/data/session.class.php');
                    break;
            }
        }
    }
    $GLOBALS['HCMS_CORE'] = new Core();
