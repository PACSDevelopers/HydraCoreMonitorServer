<?hh
namespace HCPublic\Ajax\Databases\Database;

class ProcessBackupTransferAjax extends \HC\Ajax {
	protected $settings = [
        'authentication' => true
	];

	public function init($GET = [], $POST = []) {
        $response = [];
        
		// Put all errors in an array
		$response['errors'] = [];
		if(!isset($POST['data'])){
            $response['errors']['e1'] = true;
		}

		if(count($response['errors']) == 0){
            $db = new \HC\DB();
            
            if(isset($POST['data']['id2'])) {
                $backup = $db->read('database_backups', ['databaseID', 'isLocal'], ['id' => $POST['data']['id']]);
                if($backup) {
                    $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
                    if(isset($globalSettings['backups'])) {
                        if($backup[0]['isLocal'] == 1) {
                            $before = microtime(true);
                            $dateTokens = explode('.', $before);
                            if(!isset($dateTokens[1])) {
                                $dateTokens[1] = 0;
                            }

                            $dateCreated = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
                            
                            $result = $db->write('database_transfers', ['database1ID' => $backup[0]['databaseID'], 'database2ID' => $POST['data']['id2'], 'backupID' => $POST['data']['id'], 'status' => 1, 'progress' => 0, 'creatorID' => $_SESSION['user']->getUserID(), 'dateCreated' => $dateCreated]);
                            $response = ['status' => $result];
                        } else {
                            $response['errors']['e5'] = true;
                        }
                    } else {
                        $response['errors']['e4'] = true;
                    }
                } else {
                    $response['errors']['e3'] = true;
                }
            } else {
                $databases = $db->read('databases', ['id', 'title'], ['status' => '1']);
                if($databases) {
                    $response = ['status' => 1, 'result' => $databases];
                } else {
                    $response = ['status' => 1, 'result' => []];
                } 
            }
            
		}

		$this->body = $response;
		return 1;
	}
}
