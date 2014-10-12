<?hh
namespace HCPublic;

class HomePage extends \HC\Page {
	protected $settings = [
		'views' => [
			'header' => [
				'pageName' => 'Home',
				'scss' => [
					'main' => true,
				],
				'js' => [
					'extenders' => true,
					'main' => true,
					'bootstrap-functions' => true,
					'home' => true,
				]
			],
			'body' => true,
			'footer' => true,
		],
		'authentication' => true,
	];

	public function init($GET = [], $POST = []) {
		$container = <div class="container">
            <div class="row">
                <h1>{SITE_NAME} - {$this->settings['views']['header']['pageName']}</h1>
            </div>
        </div>;

		// Add the row to the container, and render the body
		$this->body = $container;

		return 1;
	}
}
