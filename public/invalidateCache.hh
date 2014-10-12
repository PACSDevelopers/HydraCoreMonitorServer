<?hh
namespace HCPublic;

class InvalidateCachePage extends \HC\Page {
	public function init($GET = [], $POST = []) {
		$cache = new \HC\Cache();
		$db = new \HC\DB();
		$result1 = $cache->deleteAll();
		$result2 = $db->provideDefaultTableData();

		if($result1 && $result2){
			echo 'Cleared Cache';
		} else {
			echo 'Failed to clear cache';
		}

		return 1;
	}
}
