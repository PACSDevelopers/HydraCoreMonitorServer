<?hh
namespace HCPublic\Ajax\Databases\Database;

class ProcessStopBackupDatabaseAjax extends \HC\Ajax {
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
            if(isset($POST['data']['backupID'])){
                $db = new \HC\DB();
                $result = $db->update('database_backups', ['id' => $POST['data']['backupID']], ['status' => 4]);
                if($result) {
                    $response = ['status' => 1];
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
