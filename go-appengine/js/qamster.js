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
    
    $scope.running = 'Not Running';
    $scope.tracking = 'Tracking Task';
    $scope.time = 'The time thing';
    
    updateTime($scope, $http);


    
    $scope.addTask = function () {
        t = document.getElementById("taskfield").value;
        console.log("added" + t);
        $http.post('/go/addTask', t);
        updateTime($scope, $http);
    }
    
}]);

function updateTime($scope, $http) {
    $http.get('/go/timetable').
        success(function(data) {
            $scope.tasks = data;
        });
}
