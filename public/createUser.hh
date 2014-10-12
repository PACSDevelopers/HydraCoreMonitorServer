<?hh
namespace HCPublic;

class CreateUserPage extends \HC\Ajax {
	public function init($GET = [], $POST = []) {
		$var1 = $this->encryptPassword($GET['password']);
		$var2 = $this->encryptPassword($GET['password']);
		if($var1 == $var2){
				var_dump($var1);
		} else {
				echo 'Failed';
		}

		return 1;
	}

	private function encryptPassword($password){
        $encryption = new \HC\Encryption();
        $userSettings = $GLOBALS['HC_CORE']->getSite()->getSettings();
        $userSettings = $userSettings['users'];
        return $encryption->hash($password, ['salt' => $userSettings['salt']]);
	}
}
