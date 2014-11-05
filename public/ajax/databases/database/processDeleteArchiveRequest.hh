<?hh
namespace HCPublic\Ajax\Databases\Database;

class ProcessDeleteArchiveRequestAjax extends \HC\Ajax {
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
                $backup = $db->read('database_backups', ['archiveID', 'inVault'], ['id' => $POST['data']['id']]);
                if($backup) {
                    $globalSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
                    if(isset($globalSettings['backups'])) {
                        $client = \Aws\Glacier\GlacierClient::factory([
                            'key'    => $globalSettings['backups']['glacier']['key'],
                            'secret' => $globalSettings['backups']['glacier']['secret'],
                            'region' => $globalSettings['backups']['glacier']['region']
                        ]);

                        $result = $client->deleteArchive([
                            'vaultName'=> 'Backups',
                            'archiveId'=> $backup[0]['archiveID']
                        ]);

                        if($result) {
                            $before = microtime(true);
                            $dateTokens = explode('.', $before);
                            if(!isset($dateTokens[1])) {
                                $dateTokens[1] = 0;
                            }

                            $dateEdited = date('Y-m-d H:i:s', $dateTokens[0]) . '.' . str_pad($dateTokens[1], 4, '0', STR_PAD_LEFT);
                            
                            $db->update('database_backups', ['id' => $POST['data']['id']], ['inVault' => 0, 'dateEdited' => $dateEdited]);
                            $response = ['status' => 1];
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
                $response['errors']['e2'] = true;
            }
		}

		$this->body = $response;
		return 1;
	}
}
