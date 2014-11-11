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
            $this->settings['views']['body']['headerButtonsRight'] = [<a class="btn btn-primary pull-right" href="/domains/create">Create Domain</a>];
        }
        
		$db = new \HC\DB();
        
		$columns = ['ID' => 'id', 'Title' => 'title', 'URL' => 'url'];
        
        $domainsHeader = <tr></tr>;
		foreach($columns as $key => $column) {
            $domainsHeader->appendChild(<th>{$key}</th>);
		}
        $domainsHeader->appendChild(<th>Status</th>);
        
        $result = $db->read('domains', array_values($columns), ['status' => 1]);
        
        $domainsBody = <tbody></tbody>;
        if($result) {
            $result = array_reverse($result);
            foreach($result as $key => $row) {
                $domainsRow = <tr></tr>;
                foreach($row as $key2 => $value) {
                    if($key2 === 'title') {
                        $domainsRow->appendChild(<td><a href={'/domains/' . $row['id']}>{$value}</a></td>);
                    } else if($key2 === 'url') {
                        $domainsRow->appendChild(<td><a href={'http://' . $value}>{$value}</a></td>);
                    } else {
                        $domainsRow->appendChild(<td>{$value}</td>);
                    }
                }
                $domainsRow->appendChild(<td><span class="domainStatusIcon glyphicons circle_question_mark pull-right" data-id={$row['id']}></span></td>);
                $domainsBody->appendChild($domainsRow);
            }
        }
        

		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Domains</h1>
                    <div class="table-responsive">
                        <table class="table table-bordered table-striped table-hover" id="domainsTable">
                            <thead>
                                {$domainsHeader}
                            </thead>
                            {$domainsBody}
                        </table>
                    </div>
                </div>
            </div>
        </x:frag>;
        
			return 1;
	}
};
