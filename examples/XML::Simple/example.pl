{
  'logdir'      => '/var/log/foo/',
  'debugfile'   => '/tmp/foo.debug',
  'server'      => {
    'sahara'      => {
      'osversion'   => '2.6',
      'osname'      => 'solaris',
      'address'     => [ '10.0.0.101', '10.0.1.101' ]
    }, 'gobi'     => {
      'osversion'   => '6.5',
      'osname'      => 'irix',
      'address'     => '10.0.0.102'
    }, 'kalahari' => {
      'osversion'   => '2.0.34',
      'osname'      => 'linux',
      'address'     => [ '10.0.0.103', '10.0.1.103' ]
    }
  }
}
