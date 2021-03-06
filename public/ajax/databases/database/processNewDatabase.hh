<?hh
namespace HCPublic\Ajax\Databases\Database;

class ProcessNewDatabaseAjax extends \HC\Ajax {

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
            $insertKeys = [
                'databaseTitle' => 'title',
                'databaseExtIP' => 'extIP',
                'databaseIntIP' => 'intIP',
                'databaseBackupType' => 'backupType',
                'databaseBackupInterval' => 'backupInterval',
                'databaseUsername' => 'username',
                'databasePassword' => 'password'
            ];

            $isValid = true;
            $data = [];
            foreach($POST['data'] as $key => $value) {
                if(isset($insertKeys[$key])) {
                    if(is_string($value)) {
                            $data[$insertKeys[$key]] = <x:frag>{$value}</x:frag>;
                    } else {
                            $data[$insertKeys[$key]] = $value;
                    }
                }
            }

            if(isset($data['extIP'])) {
                $isIPValid = \HCMS\Database::testMySQLPort($data['extIP']);
                if(!$isIPValid) {
                    $isValid = false;
                } else {
                    $data['extIP'] = ip2long($data['extIP']);
                }
            }

            if(isset($data['intIP'])) {
                $isIPValid = \HCMS\Database::testMySQLPort($data['intIP']);
                if(!$isIPValid) {
                    $isValid = false;
                } else {
                    $data['intIP'] = ip2long($data['intIP']);
                }
            }
            
            if(!isset($data['createdBy'])) {
                $data['createdBy'] = $_SESSION['user']->getUserID();
            }

            if(!isset($data['dateCreated'])) {
                $data['dateCreated'] = time();
            }

            if(isset($data['username']) ||  isset($data['password'])) {
                $encryption = new \HC\Encryption();
            }


            if(isset($data['username'])) {
                $data['username'] = $encryption->encrypt($data['username'], 'HC_DB_U' . $data['dateCreated']);
            }

            if(isset($data['password'])) {
                $data['password'] = $encryption->encrypt($data['password'], 'HC_DB_P' . $data['dateCreated']);
            }
            
            if($isValid) {
                $database = \HCMS\Database::create($data);

                if($database){
                    $response = ['status' => 1, 'data' => $POST['data'], 'databaseID' => $database->id];
                } else {
                    $response['errors']['e2'] = true;
                }
            } else {
                $response['errors']['e3'] = true;
            }
            
		}

		$this->body = $response;
		return 1;
	}
}
