<?hh
namespace HCPublic\Data;

class ExportPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Data - Export',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
                                    'data/export' => true
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
       
        
		$this->body = <x:frag>
            <div class="container">
                <div class="row">
                    <h1>Data - Export</h1>
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
