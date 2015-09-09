<?hh
namespace HCPublic\Data;

class ExportsPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Data - Exports',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
                                    'data/exports' => true
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
        if($_SESSION['user']->hasPermission('Export')) {
            $this->settings['views']['body']['headerButtonsRight'] = [<a class="btn btn-primary pull-right" href="/data/exports/create">Create Export</a>];
        }

        $db = new \HC\DB();

        $columns = ['ID', 'Status', 'Database', 'Template', 'Schema', 'Progress', 'Action'];
        $exportsHeader = <tr></tr>;
		foreach($columns as $column) {
            $exportsHeader->appendChild(<th>{$column}</th>);
		}
        
        $exportsBody = <tbody></tbody>;
        
        $result = $db->query('SELECT
                                    `DE`.`id`, `DE`.`status`, `DE`.`progress`, `DT`.`id` as `templateID`, `DT`.`title` AS `templateTitle`, `D`.`id` as `databaseID`, `D`.`title` AS `databaseTitle`, `DE`.`schema` AS `databaseSchema`
                                FROM `data_exports` `DE`
                                LEFT JOIN `data_templates` `DT` ON (`DT`.`id` = `DE`.`templateID`)
                                LEFT JOIN `databases` `D` ON (`D`.`id` = `DE`.`databaseID`);');
        if($result) {
            $statusArray = [
                1 => <span class="glyphicons circle_question_mark"></span>,
                2 => <span class="glyphicons circle_question_mark"></span>,
                3 => <span class="glyphicons circle_ok" style="color: #53A93F;"></span>,
                4 => <span class="glyphicons circle_exclamation_mark" style="color: #E04A3F;"></span>
            ];

            $result = array_reverse($result);
            foreach($result as $key => $row) {
                $exportsRow = <tr>
                    <td>{$row['id']}</td>
                    <td>{$statusArray[$row['status']]}</td>
                    <td><a href={'/databases/' . $row['databaseID']}>{$row['databaseTitle']}</a></td>
                    <td><a href={'/data/templates/' . $row['templateID']}>{$row['templateTitle']}</a></td>
                    <td>{$row['databaseSchema']}</td>
                </tr>;

                switch($row['status']) {
                    case 2:
                        $exportsRow->appendChild(<td style="width: 30%"><div class="progress">
                              <div class="progress-bar progress-bar-striped active" role="progressbar" aria-valuenow={$row['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $row['progress'] . '%;'}>
                                <span class="sr-only">$row['progress']</span>
                              </div>
                            </div></td>);
                    break;

                    case 3:
                        $exportsRow->appendChild(<td style="width: 30%"><div class="progress">
                              <div class="progress-bar progress-bar-success" role="progressbar" aria-valuenow={$row['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $row['progress'] . '%;'}>
                                <span class="sr-only">$row['progress']</span>
                              </div>
                            </div></td>);
                    break;

                    case 4:
                        $exportsRow->appendChild(<td style="width: 30%"><div class="progress">
                              <div class="progress-bar progress-bar-danger" role="progressbar" aria-valuenow={$row['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $row['progress'] . '%;'}>
                                <span class="sr-only">$row['progress']</span>
                              </div>
                            </div></td>);
                    break;

                    default:
                        $exportsRow->appendChild(<td style="width: 30%"><div class="progress">
                              <div class="progress-bar" role="progressbar" aria-valuenow={$row['progress']} aria-valuemin="0" aria-valuemax="100" style={'width: ' . $row['progress'] . '%;'}>
                                <span class="sr-only">$row['progress']</span>
                              </div>
                            </div></td>);
                    break;
                }

                $actions = <td></td>;
                if($row['status'] != 4 && $_SESSION['user']->hasPermission('Export')) {
                    $list = <ul class="dropdown-menu" role="menu" aria-labelledby={'actionDropDown' . $row['id']}></ul>;
                    switch($row['status']) {
                        case 1:
                        case 2:
                            $list->appendChild(<li role="presentation"><a class="falseLink" role="menuitem" tabindex="-1" href="#" onclick={'stopExport(' . $row['id'] . ');'}>Stop</a></li>);
                            break;
                        case 3:
                            $list->appendChild(<li role="presentation"><a role="menuitem" tabindex="-1" href={'/downloads/exports/' . $row['id'] . '/JSON'}>Download JSON</a></li>);
                            $list->appendChild(<li role="presentation"><a role="menuitem" tabindex="-1" href={'/downloads/exports/' . $row['id'] . '/CSV'}>Download CSV</a></li>);
                            $list->appendChild(<li role="presentation"><a role="menuitem" tabindex="-1" href={'/downloads/exports/' . $row['id'] . '/XLS'}>Download XLS</a></li>);
                            $list->appendChild(<li role="presentation"><a role="menuitem" tabindex="-1" href={'/downloads/exports/' . $row['id'] . '/XLSX'}>Download XLSX</a></li>);
                            $list->appendChild(<li role="presentation"><a role="menuitem" tabindex="-1" href={'/downloads/exports/' . $row['id'] . '/YAML'}>Download YAML</a></li>);
                            break;
                        default:

                            break;
                    }

                    $actions->appendChild(<div class="dropdown">
                                              <button class="btn btn-default dropdown-toggle" type="button" id={'actionDropDown' . $row['id']} data-toggle="dropdown">
                                                <span class="caret"></span>
                                              </button>
                                                {$list}
                                            </div>);
                }

                $exportsRow->appendChild($actions);
                    
                $exportsBody->appendChild($exportsRow);
            }
        }
        
		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Data - Exports</h1>
                    <div class="table-responsive">
                        <table class="table table-bordered table-striped table-hover" id="exportsTable">
                            <thead>
                                {$exportsHeader}
                            </thead>
                            {$exportsBody}
                        </table>
                    </div>
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
