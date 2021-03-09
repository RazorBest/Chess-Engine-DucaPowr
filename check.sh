if ! command -v pip &> /dev/null 
then
    echo "You must intall pip first"
    exit
fi

if ! pip install cpplint
then
    exit
fi

cpplint --recursive * 

