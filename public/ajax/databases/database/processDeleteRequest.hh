<?hh
namespace HCPublic\Ajax\Databases\Database;

class ProcessDeleteRequestAjax extends \HC\Ajax {
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
            if(isset($POST['data']['id'])) {
                $db = new \HC\DB();
                $backup = $db->read('database_backups', ['isLocal'], ['id' => $POST['data']['id']]);
                if($backup) {
                    $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
                    if(isset($globalSettings['backups'])) {
                        if($backup[0]['isLocal'] == 1) {
                            $file = $globalSettings['backups']['archive'] . '/' . $POST['data']['id'] . '.tar.xz';
                            if(is_file($file)) {
                                unlink($file);
                            }
                            
                            $db->update('database_backups', ['id' => $POST['data']['id']], ['isLocal' => 0]);
                            $response = ['status' => 1];
                        }
                    } else {
                        $response['errors']['e4'] = true;
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
