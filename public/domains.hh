<?hh
namespace HCPublic;

class DomainsPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Domains',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'domainsTable' => true,
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
		$this->settings['views']['body']['headerButtonsRight'] = [<a class="btn btn-primary" href="/domains/create">Create Domain</a>];
		$db = new \HC\DB();
        
		$columns = ['ID', 'Title'];
		$domainsTable = new \HC\Table(['class' => 'table table-bordered table-striped table-hover', 'name' => 'domainsTable']);
		$domainsTable->openHeader();
		$domainsTable->openRow();
		foreach($columns as $column) {
				$domainsTable->column(['value' => $column]);
		}
		$domainsTable->closeRow();
		$domainsTable->closeHeader();

		$this->body = <div class="row col-lg-12">
                        <div class="row">
                                    <div class="table-responsive">
                                            {$domainsTable}
                                    </div>
                            </div>
                    </div>;
        
			return 1;
	}
};
