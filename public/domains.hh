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
									'bootstrap-functions' => true,
                                    'domainTable' => true
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
            $this->settings['views']['body']['headerButtonsRight'] = [<a class="btn btn-primary" href="/domains/create">Create Domain</a>];
        }
        
		$db = new \HC\DB();
        
		$columns = ['ID' => 'id', 'Title' => 'title', 'URL' => 'url'];
		$domainsTable = new \HC\Table(['class' => 'table table-bordered table-striped table-hover', 'name' => 'domainsTable']);
		$domainsTable->openHeader();
		$domainsTable->openRow();
		foreach($columns as $key => $column) {
				$domainsTable->column(['value' => $key]);
		}
        $domainsTable->column(['value' => 'Status']);
		$domainsTable->closeRow();
		$domainsTable->closeHeader();
        
        $result = $db->read('domains', array_values($columns), ['status' => 1]);
        $domainsTable->openBody();
        if($result) {
            $result = array_reverse($result);
            foreach($result as $key => $row) {
                $domainsTable->openRow();
                foreach($row as $key2 => $value) {
                    if($key2 === 'title') {
                        $domainsTable->column(['value' => <a href={'/domains/' . $row['id']}>{$value}</a> ]);
                    } else if($key2 === 'url') {
                        $domainsTable->column(['value' => <a href={'http://' . $value}>{$value}</a>]);
                    } else {
                        $domainsTable->column(['value' => $value]);
                    }
                }
                $domainsTable->column(['value' => <span class="domainStatusIcon glyphicons circle_question_mark pull-right" data-id={$row['id']}></span>]);
                $domainsTable->closeRow();
            }
        }
        
        $domainsTable->closeBody();

		$this->body = <x:frag>
            <div class="row col-lg-2 col-md-0 col-sm-0">
            </div>
            <div class="row col-lg-8 col-md-12 col-sm-12">
                <div class="row">
                    <h1>Domains</h1>
                    <div class="table-responsive">
                        {$domainsTable}
                    </div>
                </div>
            </div>
            <div class="row col-lg-2 col-md-0 col-sm-0">
            </div>
        </x:frag>;
        
			return 1;
	}
};
