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
									'bootstrap-functions' => true,
                                    'databaseTable' => true
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
        
		$columns = ['ID' => 'id', 'Title' => 'title', 'External IP' => 'extIP', 'Internal IP' => 'intIP'];
		$databasesTable = new \HC\Table(['class' => 'table table-bordered table-striped table-hover', 'name' => 'databasesTable']);
		$databasesTable->openHeader();
		$databasesTable->openRow();
		foreach($columns as $key => $column) {
            $databasesTable->column(['value' => $key]);
        }
        $databasesTable->column(['value' => 'Status']);
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
                    } else if($key2 === 'extIP' || $key2 === 'intIP') {
                        $databasesTable->column(['value' => long2ip($value)]);
                    } else {
                        $databasesTable->column(['value' => $value]);
                    }
                }
                $databasesTable->column(['value' => <span class="databaseStatusIcon glyphicons circle_question_mark pull-right" data-id={$row['id']}></span>]);
                $databasesTable->closeRow();
            }
        }
        
        $databasesTable->closeBody();

		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Databases</h1>
                    <div class="table-responsive">
                        {$databasesTable}
                    </div>
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
