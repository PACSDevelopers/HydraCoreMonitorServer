<?hh
namespace HCPublic\Ajax\Databases\Database;

class ProcessArchiveRequestAjax extends \HC\Ajax {
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
                $backup = $db->read('database_backups', ['archiveID', 'hasJob'], ['id' => $POST['data']['id']]);
                if($backup) {
                    if($backup[0]['hasJob'] == 0) {
                        $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
                        if(isset($globalSettings['backups'])) {
                            $client = \Aws\Glacier\GlacierClient::factory([
                                'key'    => $globalSettings['backups']['glacier']['key'],
                                'secret' => $globalSettings['backups']['glacier']['secret'],
                                'region' => $globalSettings['backups']['glacier']['region']
                            ]);
    
                            $result = $client->initiateJob(array(
                                'vaultName'=> 'Backups',
                                'Type'=> 'archive-retrieval',
                                'ArchiveId'=> $backup[0]['archiveID']
                            ));
    
                            if($result) {
                                $db->update('database_backups', ['id' => $POST['data']['id']], ['hasJob' => 1, 'jobID' => $result['jobId']]);
                                $response = ['status' => 1];
                            } else {
                                $response['errors']['e5'] = true;
                            }
                        } else {
                            $response['errors']['e4'] = true;
                        }
                    } else {
                        $response = ['status' => 2];
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
