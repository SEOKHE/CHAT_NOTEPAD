# CHATTING PROGRAM

Visual Studio의 C++에서 소켓과 MYSQL 연동을 활요한 채팅 프로그램 (MAX 10 Client)

로그인, ID찾기, PW찾기, 회원가입을 거쳐 입장
  PW는 *을 사용해서 노출되지 않도록 처리했음
  회원 가입 시에는 이름, 전화번호, 생일, ID, PW를 입력 받고 기존에 이미 DB에 등록된 사용자일 경우, 이미 등록되었다는 문구 출력(중복 가입 방지)
  ID찾기는 이름, 전화번호, 생일을 입력받고 DB에서 ID를 찾아서 출력함
  PW찾기는 이름, 전화번호, 생일, ID를 입력 받고 DB에서 등록된 회원인지 확인 후, 이미 등록된 회원이라면 PW를 변경하도록 함

내 프로필, 친구, 채팅, 설정 중 선택해서 실행

내프로필의 하위에는 내 프로필 보기(상태 메시지, 노래, 생일 출력), 상태메시지 설정/수정, 노래 설정/수정 으로 이루어져 있음

친구의 하위 항목으로는 친구 목록 확인, 생일 4자리를 통해 빠른 날짜 기준으로 검색 및 정렬이 있음

채팅은 채팅방에 입장하는 항목임
  Pc의 과부하를 방지하고자 최대 10명으로 제한(수정하고 싶을 경우 SERVER.cpp에서 '#define MAX_CLIENT 10' 을 수정하면 됨)
  SERVER.cpp를 실행시켜야 채팅방 입장 가능
  입장시, 사용자 입장 전까지 해당 채팅방의 채팅 기록이 전부 출력됨
  재입장이 가능하고 퇴장하고 싶을 경우 /quit 입력하면 됨

설정의 하위 항목에는 PW 변경, 회원 탈퇴가 있음
  회원 탈퇴를 실행하게 될 경우 사용자에게 한 번 더 탈퇴할 것인지 물어봄
  회원 탈퇴가 완료되면 2초 뒤 프로그램이 종료됨
