Set vs_version=2013    :: 2013 or 2015

goto case_%vs_version%           
:case_2013    
  cd BehaviorTrees\                        
  "%VS120COMNTOOLS%..\IDE\devenv" BehaviorTree.sln /build Release
  goto end_case
:case_2015            
  cd BehaviorTrees\                
  "%VS140COMNTOOLS%..\IDE\devenv" BehaviorTree.sln /build Release
  goto end_case
:end_case
  Copy Release\BehaviorTrees.exe .\
  cd ..
  pause