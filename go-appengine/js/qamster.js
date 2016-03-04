var app = angular.module('qamsterApp', ['ngMaterial']);

// themes
app.config(function($mdThemingProvider) {
    $mdThemingProvider.theme('default')
        .primaryPalette('green');
});


app.controller('QamsterCtrl', ['$scope', '$mdSidenav', '$http', function($scope, $mdSidenav, $http){
  $scope.toggleSidenav = function(menuId) {
    $mdSidenav(menuId).toggle();
  };
    
    $scope.tasks = [];
    updateTime($scope, $http);
}]);

function updateTime($scope, $http) {
    $http.get('/go/timetable').
        success(function(data) {
            $scope.tasks = data;
        });
}

