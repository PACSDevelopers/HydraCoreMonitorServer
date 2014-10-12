<?hh
namespace HCPublic;

class ServersPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Servers',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'serversTable' => true,
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
		$this->settings['views']['body']['headerButtonsRight'] = [<a class="btn btn-primary" href="/servers/create">Create Server</a>];
		$db = new \HC\DB();
        
		$columns = ['ID', 'Title'];
		$serversTable = new \HC\Table(['class' => 'table table-bordered table-striped table-hover', 'name' => 'serversTable']);
		$serversTable->openHeader();
		$serversTable->openRow();
		foreach($columns as $column) {
				$serversTable->column(['value' => $column]);
		}
		$serversTable->closeRow();
		$serversTable->closeHeader();

		$this->body = <div class="row col-lg-12">
                        <div class="row">
                                    <div class="table-responsive">
                                            {$serversTable}
                                    </div>
                            </div>
                    </div>;
        
			return 1;
	}
};
