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
		$this->settings['views']['body']['headerButtonsRight'] = [<a class="btn btn-primary" href="/databases/create">Create Database</a>];
		$db = new \HC\DB();
        
		$columns = ['ID', 'Title'];
		$databasesTable = new \HC\Table(['class' => 'table table-bordered table-striped table-hover', 'name' => 'databasesTable']);
		$databasesTable->openHeader();
		$databasesTable->openRow();
		foreach($columns as $column) {
				$databasesTable->column(['value' => $column]);
		}
		$databasesTable->closeRow();
		$databasesTable->closeHeader();

		$this->body = <div class="row col-lg-12">
                        <div class="row">
                                    <div class="table-responsive">
                                            {$databasesTable}
                                    </div>
                            </div>
                    </div>;
        
        return 1;
	}
};
