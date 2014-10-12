<?hh
namespace HCPublic\Ajax\Login;

class ProcessLoginAjax extends \HC\Ajax {
	public function init($GET = [], $POST = []) {
		$response = [];

		// Put all errors in an array
		$response['errors'] = [];
		if(!isset($POST['loginEmail'])){
				$response['errors']['e1'] = true;
		}

		if(!isset($POST['loginPassword'])){
				$response['errors']['e2'] = true;
		}

		if(count($response['errors']) == 0){
			if(preg_match('/^(?=.*\d)(?=.*[a-z])(?=.*[A-Z])(?!.*\s).*$/', $POST['loginPassword'])) {
				 // Check for matching details
					$db = new \HC\DB();
					$tempUser = new \HC\User(['email' => $POST['loginEmail']]);
					if($tempUser){
							if(!is_null($tempUser->getUserID())){
								$userData = $tempUser->getUserData();
								$passwordCorrect = ($userData['password'] === \HC\User::encryptPassword($POST['loginPassword']));
                                
                                if($passwordCorrect) {
                                    $_SESSION['user'] = $tempUser;
                                    $response['user'] = [];
                                    $response['user']['loggedIn'] = 1;
                                    $response['user']['f'] = $tempUser->getUserData()['firstName'];
                                    $response['user']['l'] = $tempUser->getUserData()['lastName'];
                                } else {
                                    $response['errors']['e7'] = true;
                                }
							} else {
                                $response['errors']['e6'] = true;
							}
					} else {
                        $response['errors']['e5'] = true;
					}
			} else {
				$response['errors']['e4'] = true;
			}
		} else {
            $response['errors']['e3'] = true;
        }

		$this->body = json_encode($response);
		return 1;
	}
}
