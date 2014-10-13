<?hh
namespace HCPublic;

class DatabasesPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Databases',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'databasesTable' => true,
									'bootstrap-functions' => true
							]
					],
					'body' => [
						'headerButtonsRight' => [],
					],
					'footer' => true
			],
			'forms' => true,
			'authentication' => true
	];

	public function init() {
        if($_SESSION['user']->hasPermission('Create')) {
            $this->settings['views']['body']['headerButtonsRight'] = [<a class="btn btn-primary" href="/databases/create">Create Database</a>];
        }
		
		$db = new \HC\DB();
        
		$columns = ['ID' => 'id', 'Title' => 'title', 'IP' => 'ip'];
		$databasesTable = new \HC\Table(['class' => 'table table-bordered table-striped table-hover', 'name' => 'databasesTable']);
		$databasesTable->openHeader();
		$databasesTable->openRow();
		foreach($columns as $key => $column) {
            $databasesTable->column(['value' => $key]);
        }
		$databasesTable->closeRow();
		$databasesTable->closeHeader();
        
        $result = $db->read('databases', array_values($columns), ['status' => 1]);
        $databasesTable->openBody();
        if($result) {
            $result = array_reverse($result);
            foreach($result as $key => $row) {
                $databasesTable->openRow();
                foreach($row as $key2 => $value) {
                    if($key2 === 'title') {
                        $databasesTable->column(['value' => <a href={'/databases/' . $row['id']}>{$value}</a> ]);
                    } else if($key2 === 'url') {
                        $databasesTable->column(['value' => <a href={$value}>{$value}</a>]);
                    } else if($key2 === 'ip') {
                        $databasesTable->column(['value' => long2ip($value)]);
                    } else {
                        $databasesTable->column(['value' => $value]);
                    }
                }
                $databasesTable->closeRow();
            }
        }
        
        $databasesTable->closeBody();

		$this->body = <x:frag>
            <div class="row col-lg-2 col-md-0 col-sm-0">
            </div>
            <div class="row col-lg-8 col-md-12 col-sm-12">
                <div class="row">
                    <h1>Databases</h1>
                    <div class="table-responsive">
                        {$databasesTable}
                    </div>
                </div>
            </div>
            <div class="row col-lg-2 col-md-0 col-sm-0">
            </div>
        </x:frag>;
        
        return 1;
	}
};
