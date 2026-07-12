FROM python:3.9-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY server.py .

# Server berjalan menggunakan port > 1024
EXPOSE 5001

CMD ["python", "-u", "server.py"]