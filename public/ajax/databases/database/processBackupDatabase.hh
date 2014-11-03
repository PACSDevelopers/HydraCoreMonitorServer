<?hh
namespace HCPublic\Ajax\Databases\Database;

class ProcessBackupDatabaseAjax extends \HC\Ajax {
    protected $settings = [
        'path' => '/data/archive',
        'authentication' => true
    ];

	public function init($GET = [], $POST = []) {

        $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
        if(isset($globalSettings['backups'])) {
            $this->settings = $this->parseOptions($this->settings, $globalSettings['backups']);
        }

        $response = [];
        
		// Put all errors in an array
		$response['errors'] = [];
		if(!isset($POST['data'])){
            $response['errors']['e1'] = true;
		}

		if(count($response['errors']) == 0){
            if(isset($POST['data']['databaseID'])){
                $database = new \HCMS\Database(['id' => $POST['data']['databaseID']]);
                if($database->checkExists()) {
                    $db = new \HC\DB();
                    $result = $db->query('SELECT * FROM `database_backups` `BC` WHERE `BC`.`databaseID` = ? AND `BC`.`status` IN(1, 2);', [$POST['data']['databaseID']]);
                    if($result) {
                        $response = ['status' => 2];
                    } else {
                        $before = microtime(true);
                        $dateTokens = explode('.', $before);
                        if(!isset($dateTokens[1])) {
                            $dateTokens[1] = 0;
                        }

                        $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);

                        $status = $db->write('database_backups', ['databaseID' => $POST['data']['databaseID'], 'status' => 1, 'isLocal' => 1, 'dateCreated' => $dateCreated]);
                        $response = ['status' => (int)$status];
                    }
                } else {
                    $response['errors']['e3'] = true;
                }
            } else {
                $response['errors']['e2'] = true;
            }
		}

		$this->body = $response;
		return 1;
	}
}
