<?hh
namespace HCPublic\Data;

class TemplatesPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Data - Templates',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
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
            $this->settings['views']['body']['headerButtonsRight'] = [<a class="btn btn-primary pull-right" href="/data/templates/create">Create Template</a>];
        }

        $db = new \HC\DB();

        $columns = ['ID' => 'id', 'Title' => 'title'];

        $templatesHeader = <tr></tr>;
		foreach($columns as $key => $column) {
            $templatesHeader->appendChild(<th>{$key}</th>);
		}
        
        $result = $db->read('data_templates', array_values($columns), ['status' => 1]);
        
        $templatesBody = <tbody></tbody>;
        if($result) {
            $result = array_reverse($result);
            foreach($result as $key => $row) {
                $templatesRow = <tr></tr>;
                foreach($row as $key2 => $value) {
                    if($key2 === 'title') {
                        $templatesRow->appendChild(<td><a href={'/data/templates/' . $row['id']}>{$value}</a></td>);
                    } else {
                        $templatesRow->appendChild(<td>{$value}</td>);
                    }
                }
                $templatesBody->appendChild($templatesRow);
            }
        }
        
		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Data - Templates</h1>
                    <div class="table-responsive">
                        <table class="table table-bordered table-striped table-hover" id="templatesTable">
                            <thead>
                                {$templatesHeader}
                            </thead>
                            {$templatesBody}
                        </table>
                    </div>
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
