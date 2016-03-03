angular.module('qamsterApp', []).controller('qamsterCtrl', function($scope) {
    $scope.tasks = [
        {start: '12:00', end: '12:15', title:'test1', category:'work', runtime:'15min'},
        {start: '12:15', end: '12:30', title:'test2', category:'break', runtime:'15min'},
        {start: '12:30', end: '13:00', title:'test3', category:'work', runtime:'30min'}
    ];
});

