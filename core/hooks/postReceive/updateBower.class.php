<?hh // decl


	namespace HC\Hooks\PostReceive;



	/**
	 * Class UpdateBower
	 * @package HC\Hooks\PostReceive
	 */

	class UpdateBower extends \HC\Hooks\PostReceive

	{
		/**
		 * @var array
		 */

		protected $settings;



        protected $jsComponents = [];
        protected $cssComponents = [];
        
		/**
		 * @param array $settings
		 */

		public function __construct($settings = [])

		{

			$this->settings = $settings;

		}



		/**
		 * @return bool
		 */

		public function run()

		{
            echo 'Updating Components' . PHP_EOL;

            $tempFile = '<head>
<!-- bower:js -->
<!-- endbower -->
<!-- bower:css -->
<!-- endbower -->
</head>';

            if(!is_dir(HC_TMP_LOCATION)) {
                mkdir(HC_TMP_LOCATION, 0777);
            }

            if(!is_dir(HC_TMP_LOCATION . '/bower/')) {
                mkdir(HC_TMP_LOCATION . '/bower/', 0777);
            }
            
            if(!file_exists(HC_TMP_LOCATION . '/bower/bower.html')) {
                file_put_contents(HC_TMP_LOCATION . '/bower/bower.html', $tempFile);
            }
            
            $cwd = getcwd();
            
            chdir(HC_LOCATION);
            
            if(ENVIRONMENT === 'DEV') {
                $command = 'cd ' . HC_LOCATION . ' && bower install --allow-root';
            } else {
                $command = 'cd ' . HC_LOCATION . ' && bower install --allow-root --production ';
            }
            
    
            passthru($command, $returnCode);
    
            chdir($cwd);
            
            if($returnCode === 0) {
                echo 'Updated Components' . PHP_EOL;
                
                echo 'Linking Components' . PHP_EOL;

                $cwd = getcwd();

                chdir(HC_LOCATION);

                $command = 'cd ' . HC_LOCATION . ' && grunt wiredep';

                $output = [];
                
                exec($command, $output, $returnCode);

                chdir($cwd);

                if($returnCode === 0) {
                    $this->link();
                    echo 'Linked Components' . PHP_EOL;
                    return true;
                } else {
                    echo 'Failed to Link Components' . PHP_EOL;
                }
            } else {
                echo 'Failed to update Components' . PHP_EOL;
            }
            
            return false;
		}

        public function link() {
            $html = str_replace('../../public', '', file_get_contents(HC_TMP_LOCATION . '/bower/bower.html'));
            $object = new \DomDocument();
            $object->loadHTML($html);
            $this->parseHTMLNode($object);
            
            $bowerHH = '<?hh
$hydraCoreSettings[\'pages\'][\'components\'] = [\'css\' => ' . var_export($this->cssComponents, true) . ', \'js\' => ' . var_export($this->jsComponents, true) . '];';

            file_put_contents(HC_LOCATION . '/settings/bower.hh', $bowerHH);
            return true;
        }
        
        protected function parseHTMLNode($node, $parent = null) {
            switch($node->nodeName) {
                case 'script':
                    $this->jsComponents[] = $node->getAttribute('src');
                    break;
                case 'link':
                    $this->cssComponents[] = $node->getAttribute('href');
                    break;
            }
            
            if($node->hasChildNodes()) {
                foreach ($node->childNodes as $childNode) {
                    $childNode = $this->parseHTMLNode($childNode, $node);
                }
            }
            
            return true;
        }
	}
