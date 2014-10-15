<?hh
namespace HCPublic\Ajax\Servers\Server;

class ProcessServerStatusAjax extends \HC\Ajax {
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
            if(isset($POST['data']['serverID'])){
                $server = new \HCMS\Server(['id' => $POST['data']['serverID']]);
                if($server->checkExists()) {
                    $db = new \HC\DB();
                    
                    $result = $db->read([
                        'servers' => 'S',
                        'J.SM.server_mapping' => [
                            'SM.serverID' => 'S.id'
                        ],
                        'J.D.domains' => [
                            'D.id' => 'SM.domainID'
                        ]
                    ], ['D.id' => 'domainID', 'S.id' => 'serverID', 'D.title' => 'domainTitle', 'S.title' => 'serverTitle', 'D.url', 'S.ip'], ['S.status' => 1,'D.status' => 1]);

                    if($result) {
                        foreach ($result as $row) {
                            $isValidConnection = \HCMS\Server::checkHTTP(long2ip($row['ip']), $row['url']);
                            if(!$isValidConnection) {
                                break;
                            }
                        }
                        $response = ['status' => $isValidConnection];
                    } else {
                        $response = ['status' => 1];
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
