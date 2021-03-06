<?hh
namespace HCPublic\Data;

class ImportsPage extends \HC\Page {

	protected $settings = [
			'views' => [
					'header' => [
							'pageName' => 'Data - Imports',
							'scss' => [
									'main' => true
							],
							'js' => [
									'extenders' => true,
									'main' => true,
									'bootstrap-functions' => true,
                                    'data/import' => true
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
                    <h1>Data- Imports</h1>
                </div>
            </div>
        </x:frag>;
        
        return 1;
	}
};
